#ifndef ABSTRACTLOGCOLLECTOR_H
#define ABSTRACTLOGCOLLECTOR_H

#include <QList>

template <class T>
class AbstractLogCollector
{
public:
    AbstractLogCollector(int maxSize) : m_maxSize(maxSize) {}
    virtual ~AbstractLogCollector() = default;
    QList<T> addValueAndReturnIfFull(const T &value) {
        QList<T> ret;
        if(isValid(value)) {
            m_buffer.append(value);
            if(m_buffer.size() >= m_maxSize) {
                ret = m_buffer;
                m_buffer.clear();
            }
        }
        return ret;
    }
    virtual bool isValid(const T &value) = 0;
private:
    int m_maxSize;
    QList<T> m_buffer;
};

#endif // ABSTRACTLOGCOLLECTOR_H
