/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "common.h"
#include "updatemodel.h"
#include "updatedbusproxy.h"
#include "updatejobdbusproxy.h"

#include <QNetworkReply>
#include <QObject>

struct CheckUpdateJobRet {
    QString status;
    QString jobID;
    QString jobDescription;
};

/**
 * @brief 更新日志中一个版本的信息
 *
 * 示例数据：
 * {
        "id": 1,
        "platformType": 1,
        "cnLog": "<p>中文日志</p>",
        "enLog": "<p>英文日志</p>",
        "serverType": 0,
        "systemVersion": "1070U1",
        "createdAt": "2022-08-10T17:45:54+08:00",
        "logType": 1,
        "publishTime": "2022-08-06T00:00:00+08:00"
    }
 */
struct UpdateLogItem
{
    int id = -1;
    int platformType = 1;
    int serverType = 0;
    int logType = 1;
    QString systemVersion = "";
    QString cnLog = "";
    QString enLog = "";
    QString publishTime = "";

    bool isValid() const { return -1 != id; }
};

class UpdateWorker : public QObject
{
    Q_OBJECT
public:
    explicit UpdateWorker(UpdateModel *model, QObject *parent = nullptr);
    ~UpdateWorker();
    /**
     * @brief preInitialize  更新预加载 用于获取加载前更新红点状态
     */
    void preInitialize();
    void activate();
    void deactivate();
    void setOnBattery(bool onBattery);
    void setBatteryPercentage(const BatteryPercentageInfo &info);
    void setSystemBatteryPercentage(const double &value);
    void getLicenseState();

    void setSysUpdateDownloadJobName(const QString &sysUpdateDownloadJobName);
    void setSafeUpdateDownloadJobName(const QString &safeUpdateDownloadJobName);
    void setUnknownUpdateDownloadJobName(const QString &unknownUpdateDownloadJobName);

    bool hasRepositoriesUpdates();

    void handleAtomicStateChanged(int operate, int state, QString version, QString message);
    void onAtomicUpdateFinshed(bool successed);
    void onAtomicUpdateing();

Q_SIGNALS:
    void requestInit();
    void requestActive();
    void requestRefreshLicenseState();
    void releaseNoteInstallCompleted();

public Q_SLOTS:
    void init();
    void checkForUpdates();
    void requestUpdateLog();
    void distUpgrade(ClassifyUpdateType updateType);
    void setAutoCheckUpdates(const bool autoCheckUpdates);
    void setUpdateMode(const quint64 updateMode);
    void setAutoCleanCache(const bool autoCleanCache);
    void setAutoDownloadUpdates(const bool &autoDownload);
    void setAutoInstallUpdates(const bool &autoInstall);

    void handleUpdateLogsReply(QNetworkReply *reply);
    QString getUpdateLogAddress() const;
    int isUnstableResource() const;
    void setUpdateLogs(const QJsonArray &array);

    void testMirrorSpeed();
    void checkNetselect();

    void licenseStateChangeSlot();
    void refreshLastTimeAndCheckCircle();
    void setUpdateNotify(const bool notify);

    void OnDownloadJobCtrl(ClassifyUpdateType type, int updateCtrlType);
    void onRequestOpenAppStore();
    void onClassifiedUpdatablePackagesChanged(QMap<QString, QStringList> packages);
    void onFixError(const ClassifyUpdateType &updateType, const QString &errorType);
    void onRequestLastoreHeartBeat();

private Q_SLOTS:
    void setCheckUpdatesJob(const QString &jobPath);
    void onJobListChanged(const QList<QDBusObjectPath> &jobs);
    void checkDiskSpace(const QString &jobDescription);

    void onCheckUpdateStatusChanged(const QString &value);
    void onClassityDownloadStatusChanged(const ClassifyUpdateType type, const QString &value);
    void onClassityInstallStatusChanged(const ClassifyUpdateType type, const QString &value);

    void onSysUpdateDownloadProgressChanged(double value);
    void onSafeUpdateDownloadProgressChanged(double value);
    void onUnkonwnUpdateDownloadProgressChanged(double value);

    void onSysUpdateInstallProgressChanged(double value);
    void onSafeUpdateInstallProgressChanged(double value);
    void onUnkonwnUpdateInstallProgressChanged(double value);

private:
    QMap<ClassifyUpdateType, UpdateItemInfo *> getAllUpdateInfo();
    void getItemInfo(QJsonValue jsonValue, UpdateItemInfo *itemInfo);
    void setUpdateInfo();
    void setUpdateItemDownloadSize(UpdateItemInfo *updateItem, QStringList packages);

    inline bool checkDbusIsValid();
    bool getNotUpdateState();
    void resetDownloadInfo(bool state = false);
    CheckUpdateJobRet createCheckUpdateJob(const QString &jobPath);

    void downloadAndInstallUpdates(ClassifyUpdateType updateType);

    void setDownloadJob(const QString &jobPath, ClassifyUpdateType updateType);
    void setDistUpgradeJob(const QString &jobPath, ClassifyUpdateType updateType);
    void setUpdateItemProgress(UpdateItemInfo *itemInfo, double value);

    QPointer<UpdateJobDBusProxy> getDownloadJob(ClassifyUpdateType updateType);
    QPointer<UpdateJobDBusProxy> getInstallJob(ClassifyUpdateType updateType);

    bool checkJobIsValid(QPointer<UpdateJobDBusProxy> dbusJob);
    void deleteJob(QPointer<UpdateJobDBusProxy> dbusJob);
    void deleteClassityDownloadJob(ClassifyUpdateType type);
    void deleteClassityInstallJob(ClassifyUpdateType type);
    bool checkUpdateSuccessed();
    void cleanLastoreJob(QPointer<UpdateJobDBusProxy> dbusJob);
    UpdateErrorType analyzeJobErrorMessage(QString jobDescription);
    QString getClassityUpdateDownloadJobName(ClassifyUpdateType updateType);
    void checkUpdatablePackages(const QMap<QString, QStringList> &updatablePackages);

    void backupToAtomicUpgrade();
    void updateItemInfo(const UpdateLogItem &logItem, UpdateItemInfo *itemInfo);

private:
    UpdateModel *m_model;
    QPointer<UpdateJobDBusProxy> m_checkUpdateJob;
    QPointer<UpdateJobDBusProxy> m_fixErrorJob;

    QPointer<UpdateJobDBusProxy> m_sysUpdateDownloadJob;
    QPointer<UpdateJobDBusProxy> m_safeUpdateDownloadJob;
    QPointer<UpdateJobDBusProxy> m_unknownUpdateDownloadJob;

    QPointer<UpdateJobDBusProxy> m_sysUpdateInstallJob;
    QPointer<UpdateJobDBusProxy> m_safeUpdateInstallJob;
    QPointer<UpdateJobDBusProxy> m_unknownUpdateInstallJob;

    QPointer<UpdateJobDBusProxy> m_releaseNoteInstallJob;

    QString m_sysUpdateDownloadJobName;
    QString m_safeUpdateDownloadJobName;
    QString m_unknownUpdateDownloadJobName;

    UpdateDBusProxy *m_updateInter;
    bool m_onBattery;
    double m_batteryPercentage;
    double m_batterySystemPercentage;
    QList<QString> m_updatableApps;
    QList<QString> m_updatablePackages;
    QString m_jobPath;
    qlonglong m_downloadSize;

    QMap<QString, QStringList> m_updatePackages;
    QStringList m_systemPackages;
    QStringList m_safePackages;
    QStringList m_unknownPackages;

    // 当前备份状态
    BackupStatus m_backupStatus;
    // 当前正在备份的更新分类类型
    ClassifyUpdateType m_backupingClassifyType;
    QList<ClassifyUpdateType> m_fixErrorUpdate;

    QMutex m_mutex;
    QMutex m_downloadMutex;

    QList<UpdateLogItem> m_updateLogs;
};

#endif // UPDATEWORK_H
