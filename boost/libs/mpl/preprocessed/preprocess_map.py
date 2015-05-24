
# Copyright Aleksey Gurtovoy 2001-2006
#
# Distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at 
# http://www.boost.org/LICENSE_1_0.txt)
#
# See http://www.boost.org/libs/mpl for documentation.

# $Id: preprocess_map.py 49269 2008-10-11 06:30:50Z agurtovoy $
# $Date: 2008-10-11 08:30:50 +0200 (Sat, 11 Oct 2008) $
# $Revision: 49269 $

import preprocess
import os.path

preprocess.main(
      [ "plain", "typeof_based", "no_ctps" ]
    , "map"
    , os.path.join( "boost", "mpl", "map", "aux_", "preprocessed" )
    )