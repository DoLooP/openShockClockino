class WriteBuffer;

void MM8452Q_0x1c_AcquireDataCSV(WriteBuffer *);
void MM8452Q_0x1d_AcquireDataCSV(WriteBuffer *);
void acquireData(WriteBuffer *);
void resetAcquireMetrics();
void timerStats();

extern unsigned long acquireLoop, acquireDuration, sdTotalWrite, syncWaitAVG, flushDurationAVG, flushes, sdBusyWaitAVG, sdWriteAVG, switchTimeAVG;