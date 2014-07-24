#ifndef QUPP_H_
#define QUPP_H_

struct parport_list {
	int portc;
	char portv[127][9];
};

int qusb_parse_ports(char *, struct parport_list *);

#endif /* QUPP_H_ */
