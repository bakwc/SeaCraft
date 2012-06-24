#include "Model.h"

Model::Model()
{
    myField = new Field;
    enemyField = new Field;
    state=ST_PLACING_SHIPS;
}

Model::~Model()
{
    delete myField;
    delete enemyField;
}

Cell Model::getMyCell(int x, int y) const
{
    return myField->getCell(x,y);
}

void Model::setMyCell(int x, int y, Cell cell)
{
    myField->setCell(x,y,cell);
}

QString Model::getMyField() const
{
    return myField->getField();
}

Cell Model::getEnemyCell(int x, int y) const
{
    return enemyField->getCell(x,y);
}

void Model::setEnemyCell(int x, int y, Cell cell)
{
    enemyField->setCell(x,y,cell);
}

QString Model::getEnemyField() const
{
    return enemyField->getField();
}

State Model::getState() const
{
    return state;
}

void Model::setState(State st)
{
    state=st;
}

void Model::setLogin(const QString& str)
{
    login=str;
}

void Model::setPassword(const QString& str)
{
    pass=str;
}

QString Model::getLogin() const
{
    return login;
}
QString Model::getPassword() const
{
    return pass;
}
