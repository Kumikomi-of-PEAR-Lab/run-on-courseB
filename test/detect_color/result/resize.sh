#!/usr/bin/env bash
#! -*- coding: utf-8 -*-

cnt=1
for file in `ls ../image`; do
    echo $file $cnt
    ../build/main  ../image/${file} detect_${cnt}.jpg
    let cnt++
done

