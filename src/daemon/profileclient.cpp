/**********************************************************************
 * Copyright 2015 Arto Jalkanen
 *
 * This file is part of RingingRestorer.
 *
 * RingingRestorer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RingingRestorer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RingingRestorer.  If not, see <http://www.gnu.org/licenses/>
**/
#include <QtCore/QStringList>
#include <QtDBus/QtDBus>

#include "profileclient.h"

#define PROFILED_SERVICE "com.nokia.profiled"
#define PROFILED_PATH "/com/nokia/profiled"
#define PROFILED_INTERFACE "com.nokia.profiled"
#define PROFILED_GET_PROFILES "get_profiles"
#define PROFILED_GET_PROFILE  "get_profile"
#define PROFILED_SET_PROFILE "set_profile"
#define PROFILED_GET_VALUE "get_value"
#define PROFILED_SET_VALUE "set_value"
#define PROFILED_PROFILE_CHANGED "profile_changed"
// The key for accessing Harmattan's profile type under profile
#define PROFILED_TYPE_VALUE "ringing.alert.type"
#define PROFILED_VOLUME_VALUE "ringing.alert.volume"

ProfileClient::ProfileClient(QObject *parent) : QObject(parent)
{
    if (!QDBusConnection::sessionBus().connect("", "",
                                               PROFILED_INTERFACE, PROFILED_PROFILE_CHANGED,
                                               this, SLOT(profileChanged(bool, bool, QString))))
    {
        qDebug("Failed to connect dbus signal isValid: %d", QDBusConnection::sessionBus().lastError().isValid());
        qDebug("Failed to connect dbus signal type: %d", QDBusConnection::sessionBus().lastError().type());
        qDebug("Failed to connect dbus signal: %s", qPrintable(QDBusConnection::sessionBus().lastError().name()));
    }
}

ProfileClient::~ProfileClient()
{
    if (!QDBusConnection::sessionBus().disconnect(PROFILED_SERVICE, PROFILED_PATH,
                                               PROFILED_INTERFACE, PROFILED_PROFILE_CHANGED, this, SLOT(profileChanged(bool, bool, QString))))
    {
        qDebug("Failed to disconnect dbus signal: %s", qPrintable(QDBusConnection::sessionBus().lastError().message()));
    }
}

QStringList
ProfileClient::getProfiles() const
{
    QDBusInterface dbus_iface(PROFILED_SERVICE, PROFILED_PATH,
                              PROFILED_INTERFACE);

    QDBusReply<QStringList> reply = dbus_iface.call(PROFILED_GET_PROFILES);
    QStringList profiles = reply.value();
    // For testing on Desktop without profiled running
#ifndef __arm__
    profiles.append("Profile 1");
    profiles.append("Profile 2");
    profiles.append("Profile 3");
    profiles.append("general");
#endif

    return profiles;
}

QString
ProfileClient::getProfile() const
{
    QDBusInterface dbus_iface(PROFILED_SERVICE, PROFILED_PATH,
                              PROFILED_INTERFACE);

    QDBusReply<QString> reply = dbus_iface.call(PROFILED_GET_PROFILE);
    QString profile = reply.value();
    // For testing on Desktop without profiled running
#ifndef __arm__
    profile = "general";
#endif

    return profile;
}

bool
ProfileClient::setProfile(const QString &profileName)
{
    QDBusInterface dbus_iface(PROFILED_SERVICE, PROFILED_PATH,
                              PROFILED_INTERFACE);

    qDebug("Setting profile to %s", profileName.toLatin1().constData());
    QDBusReply<bool> reply = dbus_iface.call(PROFILED_SET_PROFILE, profileName);
    if (reply.isValid()) {
        bool replyValue = reply.value();
        qDebug("Return value %d", replyValue == true);
        return replyValue;
    } else {
        qDebug("ProfileClient::getProfileType returned invalid reply for profile %s", qPrintable(profileName));
    }
    return false;
}

bool
ProfileClient::setProfileVolume(const QString &profileName, int volume)
{
    QDBusInterface dbus_iface(PROFILED_SERVICE, PROFILED_PATH,
                              PROFILED_INTERFACE);

    qDebug("Setting profile %s to volume %d", profileName.toLatin1().constData(), volume);
    QDBusReply<bool> reply = dbus_iface.call(PROFILED_SET_VALUE, profileName,
                                             PROFILED_VOLUME_VALUE, QString::number(volume));
    if (reply.isValid()) {
        bool replyValue = reply.value();
        qDebug("Return value %d", replyValue == true);
        return replyValue;
    } else {
        qDebug("ProfileClient::setProfileVolume returned invalid reply for profile %s", qPrintable(profileName));
    }
    return false;
}

QString
ProfileClient::getProfileType(const QString &profile) const {
    QDBusInterface dbus_iface(PROFILED_SERVICE, PROFILED_PATH,
                              PROFILED_INTERFACE);

    QDBusReply<QString> reply = dbus_iface.call(PROFILED_GET_VALUE, profile, PROFILED_TYPE_VALUE);

    if (reply.isValid()) {
        return reply.value();
    } else {
        qDebug("ProfileClient::getProfileType returned invalid reply for profile %s", qPrintable(profile));
    }

#ifndef __arm__
    return profile == "general" ? "Ringing" : profile + " Type";
#endif

    return QString();
}

void
ProfileClient::profileChanged(bool changed, bool active, QString profile)
{
    if (changed && active) {
        qDebug("Profile changed %d/%d/%s", changed, active, qPrintable(profile));
        emit profileChanged(profile);
    }
}

