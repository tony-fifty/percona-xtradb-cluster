--let $wait_timeout = 30
--let $wsrep_on_orig = `SELECT @@wsrep_on`
SET SESSION wsrep_on = 0;
--let $wait_condition = SELECT 1 FROM performance_schema.global_status WHERE VARIABLE_NAME = 'wsrep_debug_sync_waiters' AND VARIABLE_VALUE = '$galera_sync_point'
--source include/wait_condition.inc
--eval SET SESSION wsrep_on = $wsrep_on_orig
