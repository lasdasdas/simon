/*
 *   Copyright (C) 2010 Peter Grasch <peter.grasch@bedahr.org>
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

#ifndef SIMON_SIMONDSTREAMERCLIENT_H_0AC60651BE6A419EA6256220815A2AAD
#define SIMON_SIMONDSTREAMERCLIENT_H_0AC60651BE6A419EA6256220815A2AAD

#include "simonsender.h"
#include "simondstreamer_export.h"
#include <simonsound/soundinputclient.h>
#include <simonsound/simonsound.h>
#include <QObject>

class CompoundCondition;
class VADSoundProcessor;

class SIMONDSTREAMER_EXPORT SimondStreamerClient : public QObject, public SoundInputClient
{
  Q_OBJECT

  private:
    qint8 id;
    QByteArray currentSample;

    bool m_isRunning;
    SimonSender *sender;
    VADSoundProcessor *vad;
    
    enum StreamingState_ {
      Idle=1,
      ContextReady=2,
      VADReady=4,
      Streaming=8
    };
    Q_DECLARE_FLAGS(StreamingState, StreamingState_)
    StreamingState streamingState;
    
    CompoundCondition* m_condition;

    void inputStateChanged(SimonSound::State state);
    
    bool shouldStream(StreamingState state) const;

  signals:
    void clippingOccured();
    void started();
    void stopped();

  private slots:
    void conditionChanged();
    
  public:
    SimondStreamerClient(qint8 id, SimonSender *sender, SimonSound::DeviceConfiguration device, QObject *parent=0);
    bool stop();
    bool start();
    bool isRunning();

    void processPrivate(const QByteArray& data, qint64 currentTime);
};

#endif
