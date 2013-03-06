/*
 * configPanelParams.h
 *
 *  Created on: Feb 14, 2013
 *      Author: brent
 */

#ifndef CONFIGPANELPARAMS_H_
#define CONFIGPANELPARAMS_H_

#include "appOptions.h"
#include "raceCapture/raceCaptureConfig.h"

class ConfigPanelParams {
	public:
		ConfigPanelParams() : comm(NULL), config(NULL), appOptions(NULL) {}
		ConfigPanelParams(RaceAnalyzerComm *comm, RaceCaptureConfig *config, AppOptions *options):
				comm(comm),config(config), appOptions(options){}
		RaceAnalyzerComm *comm;
		RaceCaptureConfig *config;
		AppOptions		*appOptions;
};


#endif /* CONFIGPANELPARAMS_H_ */
