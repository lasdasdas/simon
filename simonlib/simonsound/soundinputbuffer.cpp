/*   Copyright (C) 2010 Grasch Peter <grasch@simon-listens.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "soundinputbuffer.h"
#include "simonsoundinput.h"
#include <QMutexLocker>
#include <KDebug>

SoundInputBuffer::SoundInputBuffer(SimonSoundInput* input): SoundBuffer(input),
  m_input(input)
{
  start(QThread::HighestPriority); // make sure we don't lose samples on slow maschines
}

void SoundInputBuffer::write(const char *toWrite, qint64 len)
{
  //kDebug() << "Buffering data";
  killLock.lock();
  m_buffer.append(toWrite, len);
  killLock.unlock();
  m_bufferLock.release(len);
}

void SoundInputBuffer::run()
{
  while (m_shouldBeRunning)
  {
    killLock.lock();
    //fill buffer to buffer length
    int bufferSize = m_input->bufferSize();
    killLock.unlock();

    while (!m_bufferLock.tryAcquire(bufferSize, 50)) {
      if (!m_shouldBeRunning) {
        deleteLater();
        return;
      }
    }

    killLock.lock();
    m_input->processData(m_buffer.left(bufferSize));

    m_buffer.remove(0, bufferSize);
    if (m_buffer.size() > 20*bufferSize) // drop data as a last resort
      m_buffer.resize(20*bufferSize);

    //m_buffer = m_buffer.mid(bufferSize);
    killLock.unlock();
    //kDebug()  << "Local buffer size: " << m_buffer.length();
  }
}

void SoundInputBuffer::stop()
{
  QMutexLocker lock(&killLock);
  m_shouldBeRunning = false;
  kDebug() << "Set should be running to false";
  //wait();
}

SoundInputBuffer::~SoundInputBuffer()
{
}

