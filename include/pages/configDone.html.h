//Generated from WEBsources/configDone.html
#ifndef WEBconfigDone_html_H
#define WEBconfigDone_html_H

unsigned char configDone_html[] = {0x1f,0x8b,0x08,0x00,0xac,0x8e,0x49,0x63,0x02,0xff,0x5d,0x91,0x4d,0x6f,0xc3,0x20,0x0c,0x86,0xef,0xf9,0x15,0x1e,0xe7,0xb5,0xac,0x77,0xc8,0x61,0x6d,0xa5,0x1d,0x36,0xad,0xd2,0x72,0xd9,0x91,0x82,0xdb,0x58,0x25,0x10,0xc5,0x5e,0xba,0xfc,0xfb,0xe5,0xa3,0xd3,0x3e,0x7c,0xc1,0xc0,0xc3,0x23,0xbf,0xc2,0xdc,0xed,0x5e,0xb7,0xd5,0xfb,0x61,0x0f,0x4f,0xd5,0xcb,0x73,0x59,0x98,0x5a,0x9a,0x58,0x16,0xe3,0x8a,0x2e,0x94,0x05,0x8c,0x65,0x1a,0x14,0x07,0xc9,0x35,0x68,0x55,0x4f,0x78,0x6d,0x73,0x27,0x0a,0x7c,0x4e,0x82,0x49,0xac,0xba,0x52,0x90,0xda,0x06,0xec,0xc9,0xe3,0x6a,0xde,0xdc,0x03,0x25,0x12,0x72,0x71,0xc5,0xde,0x45,0xb4,0x9b,0xf5,0x83,0x02,0x7d,0x93,0x45,0x4a,0x17,0xe8,0x30,0x5a,0xc5,0x32,0x44,0xe4,0x1a,0x71,0xb4,0xc9,0xd0,0x8e,0x76,0xc1,0x4f,0xd1,0x9e,0x59,0x41,0xdd,0xe1,0xc9,0xaa,0xfd,0xdb,0xa1,0x43,0xe6,0xbc,0x9e,0xcf,0x1a,0x0c,0xe4,0xac,0x72,0x31,0xce,0x36,0xa3,0x97,0x11,0x0b,0x73,0xcc,0x61,0xb8,0xd9,0xeb,0xcd,0xd2,0x4c,0xb5,0xcd,0xe9,0x44,0xe7,0x8f,0xce,0x09,0xe5,0x04,0xbb,0x9c,0x70,0x61,0xf4,0x37,0x64,0x02,0xf5,0x40,0xc1,0xaa,0x49,0xf0,0x18,0xb3,0xbf,0xa8,0x9f,0xd7,0xf3,0xa5,0x8f,0x8e,0xd9,0x2a,0x3f,0x9b,0x8e,0xff,0x88,0x99,0x6a,0x4b,0xa8,0x6a,0x04,0x1e,0x58,0xb0,0x01,0xe2,0x31,0x1a,0x8b,0xeb,0x84,0xd2,0xf9,0x2f,0xa8,0xdb,0x5f,0x6e,0x3d,0xca,0x6f,0x33,0x2c,0xad,0xd1,0x4b,0x88,0x29,0xd5,0xf4,0x01,0x5f,0x57,0xa3,0x0d,0x42,0x97,0x01,0x00,0x00};
unsigned int configDone_html_len =  269 ;



#define DEF_HANDLE_configDone_html  server->on("/configDone.html", HTTP_GET, std::bind (&WebInterface::handleFile, this, std::placeholders::_1,"text/html;charset=UTF-8",configDone_html,configDone_html_len));
#endif