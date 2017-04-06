#!/bin/sh

git log --pretty=format:"%an <%ae>" | sort | uniq > gammaray/authors
