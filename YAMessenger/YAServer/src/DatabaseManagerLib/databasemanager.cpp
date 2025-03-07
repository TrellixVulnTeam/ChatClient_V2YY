#include "stdafx.h"
#include "databasemanager.h"



DatabaseManager::DatabaseManager::DatabaseManager()
{
    if(QSqlDatabase::contains(QSqlDatabase::defaultConnection))
    {
        m_db = QSqlDatabase::database();
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
    }
}

DatabaseManager::DatabaseManager::~DatabaseManager()
{
    m_db.close();
}

void DatabaseManager::DatabaseManager::SetLogFile(std::shared_ptr<QFile> t_pLogFile)
{
    m_pLogFile = t_pLogFile;
}

bool DatabaseManager::DatabaseManager::ConnectToDataBase()
{

    m_db.setDatabaseName("ServerDB.db");

    if(m_db.open())
    {
        qDebug()<<"DB open";
    }
    else
    {
        //write error in log file
        WriteToLogFile(m_db.lastError().text());
        return false;
    }

    QSqlQuery qry(m_db);


    if(!m_db.tables().contains(QLatin1String("Users")))
    {
        qry.prepare("CREATE TABLE Users (id INTEGER PRIMARY KEY, Username VARCHAR, Password VARCHAR, Name VARCHAR, Surname VARCHAR);");
        if(!qry.exec())
        {
            //write error in log file
            WriteToLogFile(qry.lastError().text());
            return false;
        }
    }

    return true;
}

bool DatabaseManager::DatabaseManager::IsUsernameBusy(const QString &Username)
{
    QSqlQuery qry(m_db);

    qry.prepare("SELECT Username FROM Users WHERE Username = :name");
    qry.bindValue(":name", Username);
    if(qry.exec())
    {
        if(qry.next())
        {
            qDebug()<<"Username is busy!";
            return true;
        }
        else
            return false;
    }

    //write error in log file
    WriteToLogFile(m_db.lastError().text());
    return true;

}

bool DatabaseManager::DatabaseManager::WriteToDataBase(QString Username, const QByteArray& Password, ClientInfo::ClientInfo UInfo)
{
    if(IsUsernameBusy(Username))
    {
        return false;
    }

    QSqlQuery qry(m_db);

    qry.prepare("INSERT INTO Users (Username, Password, Name, Surname) Values (:username,:pass, :name, :surname)");
    qry.bindValue(":username", Username);
    qry.bindValue(":pass", Password.toHex());
    qry.bindValue(":name", UInfo.name);
    qry.bindValue(":surname", UInfo.surName);

    if(qry.exec())
    {
        qDebug()<<"Registration succesed";
        return true;
    }
    else
    {
        //write error in log file
        WriteToLogFile(m_db.lastError().text());
        return false;
    }
}


bool DatabaseManager::DatabaseManager::IsCorrectLogin(QString Username, const QByteArray &Password)
{
    QSqlQuery qry(m_db);

    qry.prepare("SELECT Username, Password From Users WHERE Username= :name AND Password= :pass;");
    qry.bindValue(":name", Username);
    qry.bindValue(":pass", Password.toHex());
    if(qry.exec())
    {
        if(qry.next())
        {
            return true;
        }
        qDebug()<<"Wrong Username or Password";
        return false;
    }

    //write error in log file
    WriteToLogFile(m_db.lastError().text());

    return false;
}

void DatabaseManager::DatabaseManager::WriteToLogFile(const QString &t_errorMsg)
{
    if(m_pLogFile->isOpen())
    {
        QByteArray arr;
        arr.append(QDateTime::currentDateTime().toString() + " " + t_errorMsg + "\r\n");
        m_pLogFile->write(arr);
    }
}


QString DatabaseManager::DatabaseManager::GetLastError() const
{
    return this->m_db.lastError().text();
}

void DatabaseManager::DatabaseManager::FillMapUsername(QMap<QString, ClientInfo::ClientInfo> &map)
{
    QSqlQuery qry(this->m_db);
    ClientInfo::ClientInfo tempUIS;
    qry.prepare("SELECT Username, Name, Surname FROM Users");
    if(qry.exec())
    {
        while(qry.next())
        {
            tempUIS.name = qry.value(1).toString();
            tempUIS.surName = qry.value(2).toString();
            tempUIS.pClientSocket = nullptr;
            map.insert(qry.value(0).toString(), tempUIS);
        }
    }
    else
    {
        //write errror in log file
        WriteToLogFile(qry.lastError().text());
    }
}

QSqlDatabase DatabaseManager::DatabaseManager::GetDB()
{
    return m_db;
}



