#ifndef DOWNTHREAD_H
#define DOWNTHREAD_H

#include <QObject>
#include <QThread>
#include "ch341.h"

class DownThread : public QObject
{
    Q_OBJECT
public:
    explicit DownThread(QObject *parent = 0);

    //线程处理函数
    void ThreadCommDownload(std::shared_ptr<Ch341> ch341, int channel);
    void TreadCommReadFw(std::shared_ptr<Ch341> ch341, int channel, int len);

signals:
    void MythreadChange(bool state);
    void SendCommDownloadProgress(int per, QString fw, QString type, int channel, bool ret);
    void SendCommReadProgress(int per, QString fw, QString type, int channel, bool ret);

public slots:
};

#endif // DOWNTHREAD_H
