/*
 *   Copyright (C) 2011 Adam Nash <adam.t.nash@gmail.com>
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

#ifndef SAMPLEGROUPITEMDELEGATE_H
#define SAMPLEGROUPITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "simoncontextdetection/samplegroupcondition.h"

/**
 *	@class SampleGroupItemDelegate
 *	@brief The SampleGroupItemDelegate class provides an item delegate for editting sample groups
 *
 *      The SampleGroupItemDelegate allows the user to select a sample group from ones that exist in the training,
 *      or enter a new one.
 *
 *      \sa ContextManager, TrainingManager
 *
 *	@version 0.1
 *	@date 03.05.2012
 *	@author Adam Nash
 */

class SampleGroupItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit SampleGroupItemDelegate(SampleGroupCondition* sampleGroupCondition, QObject *parent = 0);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
    SampleGroupCondition* m_sampleGroupCondition;

};

#endif // SAMPLEGROUPITEMDELEGATE_H
