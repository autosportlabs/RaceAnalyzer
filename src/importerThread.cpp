#include "importerThread.h"
#include "logging.h"

wxString ImportWizardParams::GetDefaultDatalogName(){
	return "Datalog " + wxDateTime::Now().FormatDate();
}


ImporterThread::ImporterThread() :
		wxThread(),
		m_params(NULL),
		m_owner(NULL)
{ }

void ImporterThread::SetParams(ImportWizardParams *params, wxWindow *owner){
	m_params = params;
	m_owner = owner;
}

wxThread::ExitCode ImporterThread::Entry(){

	DatalogStore *store = m_params->datalogStore;

	wxCommandEvent resultEvent( IMPORT_RESULT_EVENT, ID_IMPORT_WIZ_IMPORT_RESULT );
	try{

		wxArrayInt importChannelIds;

		DatalogChannels existingChannels;
		DatalogChannelTypes existingChannelTypes;

		store->GetAllChannels(existingChannels);
		store->GetChannelTypes(existingChannelTypes);

		DatalogChannels &importChannels = m_params->datalogChannels;
		DatalogChannelTypes &importChannelTypes = m_params->datalogChannelTypes;

		//blend channels together
		for (DatalogChannels::iterator it = importChannels.begin(); it != importChannels.end(); ++it){

			DatalogChannel &importChannel = it->second;

			if (importChannel.enabled){
				VERBOSE(FMT("channel enabled %s", importChannel.name));
				//need to add the channel?
				if (existingChannels.find(importChannel.name) == existingChannels.end()){

					wxString importChannelType = importChannel.type;
					//need to add the channel Type?
					if (existingChannelTypes.find(importChannelType) == existingChannelTypes.end()){
						existingChannelTypes[importChannelType] = importChannelTypes[importChannelType];
					}
					existingChannels[importChannel.name] = importChannel;
				}
			}
			else{
				VERBOSE(FMT("channel skipped %s", importChannel.name));
			}
		}

		store->SaveChannelTypes(existingChannelTypes);
		store->SaveChannels(existingChannels);

		store->ImportDatalog(m_params->datalogFilePath,
				m_params->datalogName,
				m_params->datalogDesc,
				importChannels,
				importChannelTypes,
				this);
		resultEvent.SetInt(1);
	}
	catch(DatastoreException &e){
		resultEvent.SetInt(0);
		resultEvent.SetString(e.GetMessage());
	}
	catch(FileAccessException &e){
		resultEvent.SetInt(0);
		resultEvent.SetString(e.GetMessage());
	}
	catch(...){
		resultEvent.SetInt(0);
		resultEvent.SetString("An unspecified error occurred");
	}
	m_owner->GetEventHandler()->AddPendingEvent(resultEvent);
	return NULL;
}

void ImporterThread::UpdateProgress(int completePercent){
	wxCommandEvent event( IMPORT_PROGRESS_EVENT, ID_IMPORT_WIZ_IMPORT_PROGRESS );
	event.SetInt(completePercent);
	m_owner->GetEventHandler()->AddPendingEvent(event);
}
