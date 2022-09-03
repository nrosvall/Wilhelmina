#include "IdleFilter.h"

IdleFilter::IdleFilter(QObject* parent, int interval) : QObject(parent)
{
    m_Interval = interval;
    m_Timer = new QTimer(this);
    m_Timer->start(m_Interval);
    connect(m_Timer, SIGNAL(timeout()), this, SLOT(Timeout()));
}

IdleFilter::~IdleFilter() {
    delete m_Timer;
}

int IdleFilter::Interval() {
    return m_Interval;
}

void IdleFilter::setClient(QMainWindow* client) {
    m_Client = client;
}

bool IdleFilter::eventFilter(QObject* obj, QEvent* ev)
{
    if (ev->type() != QEvent::Timer)
        m_Timer->start(m_Interval);

    return QObject::eventFilter(obj, ev);
}

void IdleFilter::Timeout()
{
    if (m_Client) {
        m_Client->showMinimized();
    }

    emit dummy_userInactive();
}
