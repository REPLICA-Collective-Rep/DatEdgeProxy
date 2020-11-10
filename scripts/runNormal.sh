#!/bin/bash
cd "${0%/*}"

DISPLAY=:0 ../bin/DatEdgeProxy --proxy --osc_ip "192.168.0.143"

