#!/bin/bash

START=""
END=""
INTERVAL=""

CMD="ssh smalldragon@155.230.34.228 -p 6000"

usage () {
    echo "Usage: $0 [-s start delay (in ms)] [-e end delay (in ms)] [-i interval (in ms)]" 1>&2
    exit 1
}

while getopts ":s:e:i:" o; do
    case "${o}" in
        s)
            START="${OPTARG}"
            ;;
        e)
            END="${OPTARG}"
            ;;
        i)
            INTERVAL="${OPTARG}"
            ;;
        *)
            usage
            ;;
    esac
done

shift $((OPTIND-1))

if [ -z "${START}" ] || [ -z "${END}" ] || [ -z "${INTERVAL}" ]; then
    usage
fi

echo "${CMD} \"touch hello\""

s=$((START))
while [ "${s}" -lt "${END}" ]; do
    exec "${CMD} \"sudo tc qdisc change dev ens33 root netem delay ${s}ms\""
    exec 

    s=$((s + INTERVAL))
done