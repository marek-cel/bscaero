/****************************************************************************//*
 * Copyright (C) 2021 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/

#include <gui/DialogSection.h>
#include <ui_DialogSection.h>

////////////////////////////////////////////////////////////////////////////////

DialogSection::DialogSection( QWidget *parent ) :
    QDialog ( parent ),
    _ui ( new Ui::DialogSection )
{
    _ui->setupUi( this );
}

////////////////////////////////////////////////////////////////////////////////

DialogSection::~DialogSection()
{
    if ( _ui ) delete _ui;
    _ui = Q_NULLPTR;
}

double DialogSection::getSpan()
{
    return _ui->spinBoxSpan->value();
}

////////////////////////////////////////////////////////////////////////////////

double DialogSection::getLE()
{
    return _ui->spinBoxLE->value();
}

////////////////////////////////////////////////////////////////////////////////


double DialogSection::getTE()
{
    return _ui->spinBoxTE->value();
}

////////////////////////////////////////////////////////////////////////////////


double DialogSection::getSlope()
{
    return _ui->spinBoxSlope->value();
}

////////////////////////////////////////////////////////////////////////////////


double DialogSection::getAngle()
{
    return _ui->spinBoxAngle->value();
}

////////////////////////////////////////////////////////////////////////////////

void DialogSection::setSpanMin( double min )
{
    _ui->spinBoxSpan->setMinimum( min );
}

////////////////////////////////////////////////////////////////////////////////

void DialogSection::setSpanMax( double max )
{
    _ui->spinBoxSpan->setMaximum( max );
}

////////////////////////////////////////////////////////////////////////////////

void DialogSection::setSpanDisabled( double disabled )
{
    _ui->labelSpan->setEnabled( !disabled );
    _ui->labelUnitSpan->setEnabled( !disabled );
    _ui->spinBoxSpan->setEnabled( !disabled );
}
