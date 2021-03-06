# Copyright (c) 2009, 2019, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have included with MySQL.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

ADD_DEFINITIONS(-DMYSQL_SERVER)

ADD_LIBRARY(pfs_server_stubs STATIC pfs_server_stubs.cc)

ADD_DEPENDENCIES(pfs_server_stubs GenError)

MACRO (PFS_ADD_TEST name)
  MYSQL_ADD_EXECUTABLE(${name}-t ${name}-t.cc ADD_TEST ${name})
  TARGET_LINK_LIBRARIES(${name}-t
    mytap perfschema mysys pfs_server_stubs strings sql_main ${ICU_LIBRARIES})
ENDMACRO()

SET(tests
 pfs_instr_class
 pfs_instr_class-oom
 pfs_instr
 pfs_instr-oom
 pfs_account-oom
 pfs_host-oom
 pfs_user-oom
 pfs_noop
 pfs
 pfs_misc
)
FOREACH(testname ${tests})
  PFS_ADD_TEST(${testname})
ENDFOREACH()

IF(WIN32)
  SET(MYSQLD_EXTRA_SOURCES ${CMAKE_SOURCE_DIR}/sql/nt_servc.cc)
ENDIF()

# We need the server libs to test the blob parser.
# Add sql_builtin.cc here, to force linkage of plugins below.
MYSQL_ADD_EXECUTABLE(pfs_connect_attr-t
  pfs_connect_attr-t.cc
  ${CMAKE_BINARY_DIR}/sql/sql_builtin.cc
  ${MYSQLD_EXTRA_SOURCES}
  EXCLUDE_ON_SOLARIS
  ADD_TEST pfs_connect_attr
)

ADD_DEPENDENCIES(pfs_connect_attr-t GenServerSource)
# We need to explicitly link in everything referenced in sql/sql_builtin.cc
TARGET_LINK_LIBRARIES(pfs_connect_attr-t
  mytap
  server_unittest_library
  ${MYSQLD_STATIC_PLUGIN_LIBS}
  )
