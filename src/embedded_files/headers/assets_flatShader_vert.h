const size_t ASSETS_FLATSHADER_VERT_SIZE = 784;
unsigned char assets_flatShader_vert[] = {0x23, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x34, 0x33, 0x30, 0x20, 0x63, 0x6f, 0x72, 0x65, 0x0d, 0x0a, 0x6c, 0x61, 0x79, 0x6f, 0x75, 0x74, 0x20, 0x28, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x30, 0x29, 0x20, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x61, 0x50, 0x6f, 0x73, 0x3b, 0x0d, 0x0a, 0x6c, 0x61, 0x79, 0x6f, 0x75, 0x74, 0x20, 0x28, 0x6c, 0x6f, 0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x31, 0x29, 0x20, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x61, 0x54, 0x65, 0x78, 0x43, 0x6f, 0x6f, 0x72, 0x64, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x6f, 0x75, 0x74, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x54, 0x65, 0x78, 0x43, 0x6f, 0x6f, 0x72, 0x64, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x70, 0x6f, 0x73, 0x3b, 0x0d, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x44, 0x69, 0x6d, 0x3b, 0x0d, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x62, 0x6f, 0x6f, 0x6c, 0x20, 0x76, 0x65, 0x72, 0x74, 0x49, 0x73, 0x4e, 0x44, 0x43, 0x3b, 0x0d, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x62, 0x6f, 0x6f, 0x6c, 0x20, 0x70, 0x6f, 0x73, 0x49, 0x73, 0x4e, 0x44, 0x43, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x28, 0x29, 0x0d, 0x0a, 0x7b, 0x0d, 0x0a, 0x09, 0x69, 0x66, 0x20, 0x28, 0x76, 0x65, 0x72, 0x74, 0x49, 0x73, 0x4e, 0x44, 0x43, 0x29, 0x0d, 0x0a, 0x09, 0x7b, 0x0d, 0x0a, 0x09, 0x09, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x28, 0x28, 0x76, 0x65, 0x63, 0x32, 0x28, 0x61, 0x50, 0x6f, 0x73, 0x2e, 0x78, 0x20, 0x2d, 0x20, 0x30, 0x2e, 0x35, 0x2c, 0x20, 0x2d, 0x61, 0x50, 0x6f, 0x73, 0x2e, 0x79, 0x20, 0x2b, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x20, 0x2b, 0x20, 0x76, 0x65, 0x63, 0x32, 0x28, 0x70, 0x6f, 0x73, 0x2e, 0x78, 0x2c, 0x20, 0x2d, 0x70, 0x6f, 0x73, 0x2e, 0x79, 0x29, 0x29, 0x20, 0x2a, 0x20, 0x32, 0x2c, 0x20, 0x30, 0x2e, 0x31, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0d, 0x0a, 0x09, 0x7d, 0x0d, 0x0a, 0x09, 0x65, 0x6c, 0x73, 0x65, 0x0d, 0x0a, 0x09, 0x7b, 0x0d, 0x0a, 0x09, 0x09, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x65, 0x64, 0x50, 0x6f, 0x73, 0x20, 0x3d, 0x20, 0x76, 0x65, 0x63, 0x32, 0x28, 0x30, 0x2e, 0x35, 0x2c, 0x20, 0x30, 0x2e, 0x35, 0x29, 0x3b, 0x0d, 0x0a, 0x09, 0x09, 0x69, 0x66, 0x20, 0x28, 0x70, 0x6f, 0x73, 0x49, 0x73, 0x4e, 0x44, 0x43, 0x29, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x65, 0x64, 0x50, 0x6f, 0x73, 0x20, 0x3d, 0x20, 0x28, 0x76, 0x65, 0x63, 0x32, 0x28, 0x61, 0x50, 0x6f, 0x73, 0x2e, 0x78, 0x2c, 0x20, 0x2d, 0x61, 0x50, 0x6f, 0x73, 0x2e, 0x79, 0x29, 0x20, 0x2b, 0x20, 0x76, 0x65, 0x63, 0x32, 0x28, 0x70, 0x6f, 0x73, 0x2e, 0x78, 0x20, 0x2a, 0x20, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x44, 0x69, 0x6d, 0x2e, 0x78, 0x2c, 0x20, 0x2d, 0x70, 0x6f, 0x73, 0x2e, 0x79, 0x29, 0x29, 0x20, 0x2b, 0x20, 0x76, 0x65, 0x63, 0x32, 0x28, 0x2d, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x44, 0x69, 0x6d, 0x2e, 0x78, 0x2c, 0x20, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x44, 0x69, 0x6d, 0x2e, 0x79, 0x29, 0x3b, 0x0d, 0x0a, 0x09, 0x09, 0x65, 0x6c, 0x73, 0x65, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x65, 0x64, 0x50, 0x6f, 0x73, 0x20, 0x3d, 0x20, 0x28, 0x76, 0x65, 0x63, 0x32, 0x28, 0x61, 0x50, 0x6f, 0x73, 0x2e, 0x78, 0x2c, 0x20, 0x2d, 0x61, 0x50, 0x6f, 0x73, 0x2e, 0x79, 0x29, 0x20, 0x2b, 0x20, 0x76, 0x65, 0x63, 0x32, 0x28, 0x70, 0x6f, 0x73, 0x2e, 0x78, 0x2c, 0x20, 0x2d, 0x70, 0x6f, 0x73, 0x2e, 0x79, 0x29, 0x29, 0x20, 0x2b, 0x20, 0x76, 0x65, 0x63, 0x32, 0x28, 0x2d, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x44, 0x69, 0x6d, 0x2e, 0x78, 0x2c, 0x20, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x44, 0x69, 0x6d, 0x2e, 0x79, 0x29, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x09, 0x09, 0x76, 0x65, 0x63, 0x32, 0x20, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x63, 0x74, 0x65, 0x64, 0x50, 0x6f, 0x73, 0x20, 0x3d, 0x20, 0x74, 0x72, 0x61, 0x6e, 0x73, 0x6c, 0x61, 0x74, 0x65, 0x64, 0x50, 0x6f, 0x73, 0x20, 0x2f, 0x20, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x44, 0x69, 0x6d, 0x3b, 0x0d, 0x0a, 0x09, 0x09, 0x09, 0x0d, 0x0a, 0x0d, 0x0a, 0x09, 0x09, 0x67, 0x6c, 0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x28, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x63, 0x74, 0x65, 0x64, 0x50, 0x6f, 0x73, 0x2c, 0x20, 0x30, 0x2e, 0x31, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0d, 0x0a, 0x09, 0x7d, 0x0d, 0x0a, 0x09, 0x0d, 0x0a, 0x09, 0x54, 0x65, 0x78, 0x43, 0x6f, 0x6f, 0x72, 0x64, 0x20, 0x3d, 0x20, 0x61, 0x54, 0x65, 0x78, 0x43, 0x6f, 0x6f, 0x72, 0x64, 0x3b, 0x0d, 0x0a, 0x7d};