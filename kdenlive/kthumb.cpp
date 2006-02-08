/***************************************************************************
                        krender.cpp  -  description
                           -------------------
  begin                : Fri Nov 22 2002
  copyright            : (C) 2002 by Jason Wood
  email                : jasonwood@blueyonder.co.uk
  copyright            : (C) 2005 Lcio Fl�io Corr�	
  email                : lucio.correa@gmail.com
  copyright            : (C) Marco Gittler
  email                : g.marco@freenet.de

***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kio/netaccess.h>
#include <kdebug.h>
#include <klocale.h>

#include "kthumb.h"
#include <mlt++/Mlt.h>

#include <qxml.h>
#include <qimage.h>



KThumb::KThumb(QObject * parent, const char *name):QObject(parent,
    name)
{
}

KThumb::~KThumb()
{
}


void KThumb::getImage(KURL url, int frame, int width, int height)
{
if (url.isEmpty()) return;
QPixmap image(width, height);

    Mlt::Producer m_producer(const_cast <
	char *>(QString((url.directory(false) + url.fileName())).ascii()));

    if (m_producer.is_blank()) {
	image.fill(Qt::black);
	emit thumbReady(frame, image);
	return;
	}
    Mlt::Filter m_convert("avcolour_space");
    m_convert.set("forced", mlt_image_rgb24a);
    m_producer.attach(m_convert);
    m_producer.seek(frame);
    uint orig_width = image.width();
    uint orig_height = image.height();

    Mlt::Frame * m_frame = m_producer.get_frame();

    if (m_frame) {
	m_frame->set("rescale", "nearest");
	uchar *m_thumb =
	    m_frame->fetch_image(mlt_image_rgb24a, orig_width, orig_height, 1);
	m_producer.set("thumb", m_thumb, orig_width * orig_height * 4,
	    mlt_pool_release);
	m_frame->set("image", m_thumb, 0, NULL, NULL);


	QImage m_image(m_thumb, orig_width, orig_height, 32, 0, 0,
	    QImage::IgnoreEndian);

	delete m_frame;
	if (!m_image.isNull())
	    image = (m_image.smoothScale(width, height));
	else
	    image.fill(Qt::black);
    }
emit thumbReady(frame, image);
}

