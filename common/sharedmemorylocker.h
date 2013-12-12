#include <QSharedMemory>

namespace GammaRay {

/** RAII helper class for locking QSharedMemory.
 * @todo this should be upstream
 */
class SharedMemoryLocker
{
public:
  explicit inline SharedMemoryLocker(QSharedMemory* shm) : m_shm(shm)
  {
    Q_ASSERT(shm);
    shm->lock();
  }

  inline ~SharedMemoryLocker()
  {
    m_shm->unlock();
  }

private:
  QSharedMemory *m_shm;
};

}