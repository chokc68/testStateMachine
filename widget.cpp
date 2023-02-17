#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QStateMachine>
#include <QFinalState>
#include <QHistoryState>
#include <QEventTransition>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    simpleButtonExample();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::simpleButtonExample()
{
    QStateMachine *machine = new QStateMachine(this);

    QState *s1 = new QState();
    QState *s11 = new QState(s1);
    QState *s12 = new QState(s1);
    QState *s13 = new QState(s1);
    s1->setInitialState(s11);
    machine->addState(s1);
    QFinalState *s2 = new QFinalState();
    s1->addTransition(ui->quitButton, &QPushButton::clicked, s2);
    machine->addState(s2);
    machine->setInitialState(s1);

    s11->assignProperty(ui->label, "text", "In state s1");
    s12->assignProperty(ui->label, "text", "In state s2");
    s13->assignProperty(ui->label, "text", "In state s3");

    s11->addTransition(ui->button, &QPushButton::clicked, s12);
    s12->addTransition(ui->button, &QPushButton::clicked, s13);
    s13->addTransition(ui->button, &QPushButton::clicked, s11);

    machine->addState(s11);
    machine->addState(s12);
    machine->addState(s13);

    connect(s13, &QState::entered, this, &Widget::slotShowEntered);
    connect(s13, &QState::exited, this, &Widget::slotShowExited);
    connect(machine, &QStateMachine::finished, QCoreApplication::instance(), &QCoreApplication::quit);

    QHistoryState *s1h = new QHistoryState(s1);
    machine->addState(s1h);
    QState *s3 = new QState();
    s3->assignProperty(ui->label, "text", "In s3");
    s1->addTransition(ui->interruptButton, &QPushButton::clicked, s3);
    QMessageBox *mbox = new QMessageBox(QMessageBox::Information, "Interrupt", "Interrupted!", QMessageBox::Ok, this);
    connect(s3, &QState::entered, mbox, &QMessageBox::exec);
    s3->addTransition(s1h);
    machine->addState(s3);

    machine->start();
}

void Widget::eventTransitionExample()
{
    QStateMachine *machine = new QStateMachine(this);

    QState *s1 = new QState();
    s1->assignProperty(ui->button, "text", "Outside");

    QState *s2 = new QState();
    s2->assignProperty(ui->button, "text", "Inside");

    QEventTransition *enterTransition = new QEventTransition(ui->button, QEvent::Enter);
    enterTransition->setTargetState(s2);
    s1->addTransition(enterTransition);

    QEventTransition *leaveTransition = new QEventTransition(ui->button, QEvent::Leave);
    leaveTransition->setTargetState(s1);
    s2->addTransition(leaveTransition);

    QState *s3 = new QState();
    s3->assignProperty(ui->button, "text", "Pressing...");

    QEventTransition *pressTransition = new QEventTransition(ui->button, QEvent::MouseButtonPress);
    pressTransition->setTargetState(s3);
    s2->addTransition(pressTransition);

    QEventTransition *releaseTransition = new QEventTransition(ui->button, QEvent::MouseButtonRelease);
    releaseTransition->setTargetState(s2);
    s3->addTransition(releaseTransition);

    machine->addState(s1);
    machine->addState(s2);
    machine->addState(s3);

    machine->setInitialState(s1);
    machine->start();
}

void Widget::slotShowEntered()
{
    ui->label2->setText("Show Entered");
}

void Widget::slotShowExited()
{
    ui->label2->setText("Show Exited");
}
