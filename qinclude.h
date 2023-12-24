/*
 *  QConsole by Davide Libenzi ( Symbian console server )
 *  Copyright (C) 2004  Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#if !defined(_QINCLUDE_H)
#define _QINCLUDE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <e32base.h>
#include <e32test.h>
#include <e32svr.h>
#include <e32cons.h>
#include <c32comm.h>
#include <f32file.h>
#include <es_sock.h>
#include <bt_sock.h>
#include <btmanclient.h>
#include <btsdp.h>
#include <utf.h>
#include <hal.h>
#include <hal_data.h>
#ifdef UIQ
#include <qikutils.h>
#endif
#include <apgcli.h>
#include "qmacros.h"
#include "qsha1.h"
#include "qutf7.h"
#include "qconsole.h"
#include "qcommands.h"


#endif

