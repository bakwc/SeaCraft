#pragma once

#include <QHostAddress>

const quint16 DEFAULT_SEARCH_INTERVAL = 3000;
const quint16 DEFAULT_SERVER_PORT = 1234;
const QHostAddress DEFAULT_SERVER_ADDRESS = QHostAddress::Any;
const QString DEFAULT_STAT_FILE = "stats";
const QString DEFAULT_AUTH_FILE = "authorized";
const QString DEFAULT_GUEST_ACCOUNT = "guest";
const quint16 DEFAULT_SHIP_SIZE = 4;
const quint16 LOGIN_LENGTH_MIN = 2;
const quint16 LOGIN_LENGTH_MAX = 16;
const quint16 PASSWORD_LENGTH_MIN = 4;
const quint16 PASSWORD_LENGTH_MAX = 32;
