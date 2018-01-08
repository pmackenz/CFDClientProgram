#include "cwe_globals.h"

cwe_globals::cwe_globals()
{

}

void cwe_globals::displayPopup(QString message, QString header)
{
    QMessageBox infoMessage;
    infoMessage.setWindowTitle(header);
    infoMessage.setText(message);
    infoMessage.setIcon(QMessageBox::Information);
    infoMessage.exec();
}

void cwe_globals::displayPopup(QString message)
{
    displayPopup(message, "Error");
}