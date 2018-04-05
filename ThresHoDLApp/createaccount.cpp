#include "createaccount.h"
#include "ui_createaccount.h"

CreateAccount::CreateAccount(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateAccount)
{
    ui->setupUi(this);

    ui->createAccountButton->setStyleSheet("border-radius: 5px; background-color: #15AEFF; padding: 10px;");
}

CreateAccount::~CreateAccount()
{
    delete ui;
}

void CreateAccount::on_createAccountButton_pressed()
{
    if (!ui->usernameLineEdit->text().isEmpty()) {
        emit createUserAccount(ui->usernameLineEdit->text(), "", "");
    } else {
        ui->warningLabel->setStyleSheet("color: red;");
        ui->warningLabel->setText("*Please enter username");
    }
}
