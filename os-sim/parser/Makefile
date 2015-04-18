CC = gcc 
CCFLAGS = -Wall -g
LIBS = -lfl -lmysqlclient -L/usr/local/lib/mysql -L/usr/lib/mysql/
INCLUDE = -I/usr/include/mysql -I/usr/local/include/mysql
DEFINE = -funroll-loops -O3 -DVERBOSE

SNORT_RULES_DIR = /usr/local/share/snort/rules/
ATTACK_RESPONSES_FILE = $(SNORT_RULES_DIR)/attack-responses.rules
#FW1_OBJECTS_FILE = objects.C

parser: parser.o calculate.o policy.o base.o host_levels.o host_assets.o net_levels.o net_assets.o fw1.o acid_net.o conf.o graph.o rrd_anomaly.o common.h
	cat $(ATTACK_RESPONSES_FILE) | ./gen_attackresponses_sids > attack_responses_sids.dat
	ls $(SNORT_RULES_DIR)/*.rules | ./readsids > sids.dat
	#cat $(FW1_OBJECTS_FILE) | ./gen_fw1_objects > fw1_objects.dat
	$(CC) -o $@ parser.o calculate.o policy.o base.o host_levels.o host_assets.o net_levels.o net_assets.o fw1.o acid_net.o conf.o graph.o rrd_anomaly.o $(LIBS)

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDE) $(DEFINE) -c $<

parser.c: parser.lex
	flex -oparser.c parser.lex

clean: 
	rm -f *.o *~ parser.c parser.h attack_responses_sids.dat sids.dat

realclean: clean
	rm -f parser 
	rm -f attack_responses_sids.dat sids.dat fw1_objects.dat 

