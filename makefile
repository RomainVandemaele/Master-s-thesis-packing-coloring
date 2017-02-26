#part cplex
SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPLEXDIR   = /opt/ibm/ILOG/CPLEX_Studio127/cplex
CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio127/concert

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

#CPLEXFLAGS = -I $(CPLEXDIR)/include -I $(CONCERTDIR)/include -L $(CPLEXLIBDIR) -lilocplex -lcplex -L $(CONCERTLIBDIR) -lconcert -lm -m64 -lpthread
CPLEXFLAGS = -I $(CPLEXDIR)/include -I $(CONCERTDIR)/include -L $(CPLEXLIBDIR) -lilocplex -lcplex -L $(CONCERTLIBDIR) -lconcert -lm -m64 -lpthread

CC  = g++
CXX = g++
RM  = rm -f
CPPFLAGS = -O3 -Wall -ansi -pedantic -DIL_STD -std=c++11 $(CPLEXFLAGS) $(DEBUG)
LDFLAGS  = -g   $(DEBUG)
#DEBUG = -pg -g -Wall -ansi -pedantic -DIL_STD
#DEBUG  = -ggdb -pg # -lefence

PACKAGE = coloring

SRCS    =  common.cpp Graph.cpp Node.cpp Cplex.cpp main.cpp
OBJS    =  $(subst .cpp,.o,$(SRCS))
FILES   =  $(SRCS)
VER     =  `date +%d%m%Y%k%M`




all: $(PACKAGE)

$(PACKAGE): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(CPLEXFLAGS) -o $(PACKAGE)


depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS)  -MM $^>>./.depend;

clean:
	$(RM) $(OBJS) $(PACKAGE)
	$(RM) *~ #*#

tar:
	@echo $(PACKAGE)-$(VER) > .package
	@$(RM) -r `cat .package`
	@mkdir `cat .package`
	@ln $(FILES) `cat .package`
	tar cvf - `cat .package` | gzip -9 > `cat .package`.tar.gz
	@$(RM) -r `cat .package` .package

zip:
	zip -9 $(PACKAGE)-$(VER).zip $(FILES)


#prof: run
	#$(PROF) $(PACKAGE) | less

run: all
	./$(PACKAGE)

include .depend
