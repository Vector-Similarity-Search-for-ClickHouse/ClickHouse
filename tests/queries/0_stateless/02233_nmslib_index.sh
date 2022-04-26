#!/usr/bin/env bash

CURDIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
# shellcheck source=../shell_config.sh
. "$CURDIR"/../shell_config.sh

$CLICKHOUSE_CLIENT --query="DROP TABLE IF EXISTS t_nsmlib_test;"

$CLICKHOUSE_CLIENT -n --query="
CREATE TABLE t_nsmlib_test
(
    id Int64,
    number Tuple(Float32, Float32, Float32),
    INDEX x (number) TYPE nmslib GRANULARITY 1
) ENGINE = MergeTree()
ORDER BY id
"

$CLICKHOUSE_CLIENT --query="
INSERT INTO t_nsmlib_test SELECT
    number AS id,
    (toFloat32(number), toFloat32(number), toFloat32(number))
FROM system.numbers
LIMIT 1000;"

# simple select
$CLICKHOUSE_CLIENT --query="SELECT * from t_nsmlib_test FORMAT JSON" | grep "rows_read"


$CLICKHOUSE_CLIENT --query="DROP TABLE t_nsmlib_test;"
