// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UTILS_H
#define UTILS_H

#include <dsysinfo.h>

#include <QFile>
#include <QLocale>
#include <QSettings>

const static QString serverEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Server-CN-%1.txt";
const static QString serverEnduserAgreement_old =
        "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Server/"
        "End-User-License-Agreement-Server-CN-%1.txt";
const static QString eulerServerEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Server-Euler-%1.txt";
const static QString homeEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Home-CN-%1.txt";
const static QString homeEnduserAgreement_old =
        "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Home/"
        "End-User-License-Agreement-Home-CN-%1.txt";
const static QString professionalEnduserAgreement_new =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Professional-CN-%1.txt";
const static QString professionalEnduserAgreement_old =
        "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Professional/"
        "End-User-License-Agreement-Professional-CN-%1.txt";
const static QString educationEnduserAgreement =
        "/usr/share/protocol/enduser-agreement/End-User-License-Agreement-Education-CN-%1.txt";

static const QStringList DCC_CONFIG_FILES{
    "/etc/deepin/dde-control-center.conf", "/usr/share/dde-control-center/dde-control-center.conf"
};

static const QMap<QString, QString> SYSTEM_LOCAL_MAP{
    { "zh_CN", "zh_CN" },
    { "zh_HK", "zh_HK" },
    { "zh_TW", "zh_TW" },
};

static const QStringList SYSTEM_LOCAL_LIST{
    "zh_CN",
    "zh_HK",
    "zh_TW",
    "ug_CN", // 维语
    "bo_CN"  // 藏语
};

inline bool isFileExist(const QString &path)
{
    QFile file(path);
    return file.exists();
}

namespace DCC_LICENSE {
using Dtk::Core::DSysInfo;

static const QString getLicensePath(const QString &filePath, const QString &type)
{
    const QString &locale{ QLocale::system().name() };
    QString lang = SYSTEM_LOCAL_LIST.contains(locale) ? locale : "en_US";

    QString path = QString(filePath).arg(lang).arg(type);
    if (isFileExist(path))
        return path;
    else
        return QString(filePath).arg("en_US").arg(type);
}

inline const QString getLicenseText(const QString &filePath, const QString &type)
{
    QFile license(getLicensePath(filePath, type));
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return buf;
}

[[maybe_unused]] inline const QString getDevelopModeLicense(const QString &filePath,
                                                            const QString &type)
{
    const QString &locale{ QLocale::system().name() };
    QString lang;
    if (SYSTEM_LOCAL_MAP.keys().contains(locale)) {
        lang = { SYSTEM_LOCAL_MAP.value(QLocale::system().name(), "en_US") };
    }

    if (lang.isEmpty()) {
        lang = { SYSTEM_LOCAL_MAP.value(QLocale::system().name(), "en_US") };
    }

    QString path = QString(filePath).arg(lang).arg(type);
    QFile license(path);
    if (!license.open(QIODevice::ReadOnly))
        return QString();

    const QByteArray buf = license.readAll();
    license.close();

    return buf;
}

[[maybe_unused]] inline void getPrivacyFile(QString &zhCNContent, QString &enUSContent)
{
    // 使用新的协议文档路径
    const QString newCNContent = "/usr/share/protocol/privacy-policy/Privacy-Policy-CN-zh_CN.md";
    const QString newENContent = "/usr/share/protocol/privacy-policy/Privacy-Policy-CN-en_US.md";

    const QString oldCNContent = "/usr/share/deepin-deepinid-client/privacy/deepinid-CN-zh_CN.md";
    const QString oldENContent = "/usr/share/deepin-deepinid-client/privacy/deepinid-CN-en_US.md";

    QFile privacyzhCNFile(newCNContent);
    zhCNContent = privacyzhCNFile.exists() ? newCNContent : oldENContent;
    QFile privacyenUSFile(newENContent);
    enUSContent = privacyzhCNFile.exists() ? newENContent : oldENContent;
    // 目前社区版的协议只放在这个路径下,后续如果修改了,再作适配
    if (DSysInfo::isCommunityEdition()) {
        zhCNContent = "/usr/share/deepin-deepinid-client/privacy/Privacy-Policy-Community/"
                      "Privacy-Policy-CN-zh_CN.md";
        enUSContent = "/usr/share/deepin-deepinid-client/privacy/Privacy-Policy-Community/"
                      "Privacy-Policy-CN-en_US.md";
    }
}

[[maybe_unused]] inline QString getUserExpContent()
{
    QString userExpContent = getLicensePath("/usr/share/protocol/userexperience-agreement/"
                                            "User-Experience-Program-License-Agreement-CN-%1.md",
                                            "");
    if (DSysInfo::isCommunityEdition()) {
        userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/"
                                        "User-Experience-Program-License-Agreement-Community/"
                                        "User-Experience-Program-License-Agreement-CN-%1.md",
                                        "");
        return userExpContent;
    }
    QFile newfile(userExpContent);
    if (false == newfile.exists()) {
        userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/"
                                        "User-Experience-Program-License-Agreement/"
                                        "User-Experience-Program-License-Agreement-CN-%1.md",
                                        "");
        QFile file(userExpContent);
        if (false == file.exists()) {
            userExpContent = getLicensePath("/usr/share/deepin-deepinid-client/privacy/"
                                            "User-Experience-Program-License-Agreement-%1.md",
                                            "");
        }
    }
    return userExpContent;
}

inline QString getEndUserAgreement()
{
    auto licenseOldGet = [](QString path) -> QString {
        const QString bodypath_old = getLicensePath(path, "");
        return QFile::exists(bodypath_old)
                ? getLicenseText(serverEnduserAgreement_old, "")
                : getLicenseText("/usr/share/deepin-deepinid-client/privacy/"
                                 "End-User-License-Agreement-%1.txt",
                                 "");
    };
    if (DSysInfo::uosType() == DSysInfo::UosType::UosServer) {
        const QString bodypath_new = getLicensePath(serverEnduserAgreement_new, "");
        return QFile::exists(bodypath_new) ? getLicenseText(serverEnduserAgreement_new, "")
                                           : licenseOldGet(serverEnduserAgreement_old);

    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosHome) {
        const QString bodypath_new = getLicensePath(homeEnduserAgreement_new, "");
        return QFile::exists(bodypath_new) ? getLicenseText(homeEnduserAgreement_new, "")
                                           : licenseOldGet(homeEnduserAgreement_old);
    } else if (DSysInfo::isCommunityEdition()) {
        return getLicenseText(
                "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-Community/"
                "End-User-License-Agreement-CN-%1.txt",
                "");
    } else if (DSysInfo::uosEditionType() == DSysInfo::UosEdition::UosEducation) {
        const QString bodypath = getLicensePath(educationEnduserAgreement, "");
        if (QFile::exists(bodypath)) {
            return getLicenseText(educationEnduserAgreement, "");
        }
    } else {
        const QString bodypath_new = getLicensePath(professionalEnduserAgreement_new, "");
        return QFile::exists(bodypath_new) ? getLicenseText(professionalEnduserAgreement_new, "")
                                           : licenseOldGet(professionalEnduserAgreement_old);
    }
    return QString();
}

inline QString getEulerEndUserAgreement()
{
    const QString bodypath_new = getLicensePath(eulerServerEnduserAgreement_new, "");
    if (QFile::exists(bodypath_new)) {
        return getLicenseText(eulerServerEnduserAgreement_new, "");
    } else {
        return getLicenseText(
                "/usr/share/deepin-deepinid-client/privacy/End-User-License-Agreement-%1.txt",
                "");
    }
}

inline QPair<QString, QString> loadLicenses()
{
    const QString title = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "title");
    const QString body = getLicenseText(":/systeminfo/gpl/gpl-3.0-%1-%2.txt", "body");
    return QPair<QString, QString>(title, body);
}
} // namespace DCC_LICENSE

template<typename T>
T valueByQSettings(const QStringList &configFiles,
                   const QString &group,
                   const QString &key,
                   const QVariant &failback)
{
    for (const QString &path : configFiles) {
        QSettings settings(path, QSettings::IniFormat);
        if (!group.isEmpty()) {
            settings.beginGroup(group);
        }

        const QVariant &v = settings.value(key);
        if (v.isValid()) {
            T t = v.value<T>();
            return t;
        }
    }

    return failback.value<T>();
}

#endif // UTILS_H
