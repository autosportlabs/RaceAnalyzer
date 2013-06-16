/*
 * logging.cpp
 *
 *  Created on: May 20, 2009
 *      Author: brent
 */
#include "logging.h"

bool loggingVerbose = false;
bool loggingInfo = false;

void EnableVerbose(bool enabled){
	loggingVerbose = enabled;
}

void EnableInfo(bool enabled){
	loggingInfo = enabled;
}

