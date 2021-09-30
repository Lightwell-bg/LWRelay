bool checkSchedule() {
    struct tm tnow = myESPTime.getTimeStruct();
    bool newRelayStatus = false; 
    uint8_t bitOn = 0;
    if (mySchedule[0].onWork) {
        bitOn = (day_byte[tnow.tm_wday]) & mySchedule[0].sched_stat;
        if (myESPTime.compTimeInt(mySchedule[0].workFrom, mySchedule[0].workTo, &tnow) && (bitOn > 0)) {
            newRelayStatus = true;
        }
    }
    if (mySchedule[1].onWork) {
        bitOn = (day_byte[tnow.tm_wday]) & mySchedule[1].sched_stat;
        if (myESPTime.compTimeInt(mySchedule[1].workFrom, mySchedule[1].workTo, &tnow) && (bitOn > 0)) {
            newRelayStatus = true;
        }
    }
    if (mySchedule[2].onWork) {
        bitOn = (day_byte[tnow.tm_wday]) & mySchedule[2].sched_stat;
        if (myESPTime.compTimeInt(mySchedule[2].workFrom, mySchedule[2].workTo, &tnow) && (bitOn > 0)) {
            newRelayStatus = true;
        }
    }
    if (mySchedule[3].onWork) {
        bitOn = (day_byte[tnow.tm_wday]) & mySchedule[3].sched_stat;
        if (myESPTime.compTimeInt(mySchedule[3].workFrom, mySchedule[3].workTo, &tnow) && (bitOn > 0)) {
            newRelayStatus = true;
        }
    }
    //if(!mySchedule[0].onWork && !mySchedule[1].onWork && !mySchedule[2].onWork && !mySchedule[3].onWork) newRelayStatus = true;
    return newRelayStatus; //if one sch is on - return ON, else OFF
}