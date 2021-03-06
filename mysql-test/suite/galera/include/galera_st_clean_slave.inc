--echo Performing State Transfer on a server that starts from a clean var directory
--echo This is accomplished by shutting down node #2 and removing its var directory before restarting it

--connection node_1
CREATE TABLE t1 (f1 CHAR(255)) ENGINE=InnoDB;
SET AUTOCOMMIT=OFF;
START TRANSACTION;
INSERT INTO t1 VALUES ('node1_committed_before');
INSERT INTO t1 VALUES ('node1_committed_before');
INSERT INTO t1 VALUES ('node1_committed_before');
INSERT INTO t1 VALUES ('node1_committed_before');
INSERT INTO t1 VALUES ('node1_committed_before');
COMMIT;

--connection node_2
SET AUTOCOMMIT=OFF;
START TRANSACTION;
INSERT INTO t1 VALUES ('node2_committed_before');
INSERT INTO t1 VALUES ('node2_committed_before');
INSERT INTO t1 VALUES ('node2_committed_before');
INSERT INTO t1 VALUES ('node2_committed_before');
INSERT INTO t1 VALUES ('node2_committed_before');
COMMIT;

--echo Shutting down server ...
--source include/shutdown_mysqld.inc

--connection node_1
--let $wait_condition = SELECT VARIABLE_VALUE = 1 FROM performance_schema.global_status WHERE VARIABLE_NAME = 'wsrep_cluster_size'
--source include/wait_condition.inc

--echo Cleaning var directory ...
--remove_files_wildcard $MYSQLTEST_VARDIR/mysqld.2/data/mtr *
--remove_files_wildcard $MYSQLTEST_VARDIR/mysqld.2/data/performance_schema *
--remove_files_wildcard $MYSQLTEST_VARDIR/mysqld.2/data/test *
--remove_files_wildcard $MYSQLTEST_VARDIR/mysqld.2/data/mysql *
--remove_files_wildcard $MYSQLTEST_VARDIR/mysqld.2/data/sys *
--remove_files_wildcard $MYSQLTEST_VARDIR/mysqld.2/data *

SET AUTOCOMMIT=OFF;
START TRANSACTION;
INSERT INTO t1 VALUES ('node1_committed_during');
INSERT INTO t1 VALUES ('node1_committed_during');
INSERT INTO t1 VALUES ('node1_committed_during');
INSERT INTO t1 VALUES ('node1_committed_during');
INSERT INTO t1 VALUES ('node1_committed_during');
COMMIT;

START TRANSACTION;
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');

--connect node_1a_galera_st_clean_slave, 127.0.0.1, root, , test, $NODE_MYPORT_1
SET AUTOCOMMIT=OFF;
START TRANSACTION;
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');

--connection node_2
--echo Starting server ...
--source include/start_mysqld.inc

--let $wait_condition = SELECT VARIABLE_VALUE = 2 FROM performance_schema.global_status WHERE VARIABLE_NAME = 'wsrep_cluster_size'
--source include/wait_condition.inc

SET AUTOCOMMIT=OFF;
START TRANSACTION;
INSERT INTO t1 VALUES ('node2_committed_after');
INSERT INTO t1 VALUES ('node2_committed_after');
INSERT INTO t1 VALUES ('node2_committed_after');
INSERT INTO t1 VALUES ('node2_committed_after');
INSERT INTO t1 VALUES ('node2_committed_after');
COMMIT;

--connection node_1
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
INSERT INTO t1 VALUES ('node1_to_be_committed_after');
COMMIT;

SET AUTOCOMMIT=OFF;
START TRANSACTION;
INSERT INTO t1 VALUES ('node1_committed_after');
INSERT INTO t1 VALUES ('node1_committed_after');
INSERT INTO t1 VALUES ('node1_committed_after');
INSERT INTO t1 VALUES ('node1_committed_after');
INSERT INTO t1 VALUES ('node1_committed_after');
COMMIT;

--connection node_1a_galera_st_clean_slave
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
INSERT INTO t1 VALUES ('node1_to_be_rollbacked_after');
ROLLBACK;

SELECT COUNT(*) = 35 FROM t1;
SELECT COUNT(*) = 0 FROM (SELECT COUNT(*) AS c, f1 FROM t1 GROUP BY f1 HAVING c NOT IN (5, 10)) AS a1;
COMMIT;
SET AUTOCOMMIT=ON;

--connection node_1
SELECT COUNT(*) = 35 FROM t1;
SELECT COUNT(*) = 0 FROM (SELECT COUNT(*) AS c, f1 FROM t1 GROUP BY f1 HAVING c NOT IN (5, 10)) AS a1;
DROP TABLE t1;
COMMIT;
SET AUTOCOMMIT=ON;
