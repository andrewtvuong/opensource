#!/bin/bash
# returns storage quota and sorts dirs by size allow you to free up space easily.
du -kd1 ~/ |sort -n && chkquota
