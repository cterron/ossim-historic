CC = gcc 
CCFLAGS = -Wall -g
LIBS = -lfl -lmysqlclient
INCLUDE = -I/usr/include/mysql
DEFINE = -funroll-loops -O3 -DVERBOSE

SNORT_RULES_DIR = /etc/snort/rules/
ATTACK_RESPONSES_FILE = $(SNORT_RULES_DIR)/attack-responses.rules
#FW1_OBJECTS_FILE = objects.C

parser: parser.o calculate.o base.o fw1.o acid_net.o conf.o common.h
	cat $(ATTACK_RESPONSES_FILE) | ./gen_attackresponses_sids > attack_responses_sids.dat
	ls $(SNORT_RULES_DIR)/*.rules | ./readsids > sids.dat
#	cat objects.C | ./gen_fw1_objects > fw1_objects.dat
	$(CC) -o $@ parser.o calculate.o base.o fw1.o acid_net.o conf.o $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDE) $(DEFINE) -c $<

parser.c: parser.lex
	flex -oparser.c --fast parser.lex

clean: 
	rm -vf *.o *~ parser.c parser.h

realclean: clean
	rm -vf parser 
	rm -vf attack_responses_sids.dat sids.dat fw1_objects.dat 

