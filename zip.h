#ifndef ZIP_H
#define ZIP_H

#include "miniz.h"
#include <stdio.h>
#include <fstream>
#include <string>
#define BUF_SIZE (1024 * 1024)
#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))
static unsigned char s_inbuf[BUF_SIZE];
static unsigned char s_outbuf[BUF_SIZE];
using namespace std;
string name;
string type;
string zipCompress(string src)
{
    int fl = src.length();
    for (int i = 0; i < fl; i++)
        if (src[i] == '\\')
        {
            src.insert(i, "\\\\");
            i += 2;
        }
    for (int i = src.length() - 1; i >= 0; i--)
        if (src[i] == '.')
        {
            name = src.substr(0, i);
            type = src.substr(i + 1);
            break;
        }
    FILE *pInfile, *pOutfile;
    char* srcChar = (char*)src.data();
    pInfile = fopen(srcChar, "rb");
    if (!pInfile)
    {
        fclose(pInfile);
        return "No file input";
    }

    // Determine input file's size.
    fseek(pInfile, 0, SEEK_END);
    long file_loc = ftell(pInfile);
    fseek(pInfile, 0, SEEK_SET);
    string compFile = name + "."+ type + ".zipc";
    char *lf = (char *)compFile.data();
    pOutfile = fopen(lf, "wb");
    if (!pOutfile)
    {
        fclose(pInfile);
        fclose(pOutfile);
        return "No file output";
    }
    z_stream stream;
    int level = Z_BEST_COMPRESSION;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = s_inbuf;
    stream.avail_in = 0;
    stream.next_out = s_outbuf;
    stream.avail_out = BUF_SIZE;
    // Compression.
    unsigned int infile_remaining = (unsigned int)file_loc;

    if (deflateInit(&stream, level) != Z_OK)
    {
        fclose(pInfile);
        fclose(pOutfile);
        return "CompressInit fail";
    }

    for ( ; ; )
    {
        int status;
        if (!stream.avail_in)
        {
            unsigned int n = my_min(BUF_SIZE, infile_remaining);
            if (fread(s_inbuf, 1, n, pInfile) != n)
            {
                fclose(pInfile);
                fclose(pOutfile);
                return "Input file error";
            }
            stream.next_in = s_inbuf;
            stream.avail_in = n;
            infile_remaining -= n;
        }
        status = deflate(&stream, infile_remaining ? Z_NO_FLUSH : Z_FINISH);
        if ((status == Z_STREAM_END) || (!stream.avail_out))
        {
            unsigned int n = BUF_SIZE - stream.avail_out;
            if (fwrite(s_outbuf, 1, n, pOutfile) != n)
            {
                fclose(pInfile);
                fclose(pOutfile);
                return "Output file error";
            }
            stream.next_out = s_outbuf;
            stream.avail_out = BUF_SIZE;
        }

        if (status == Z_STREAM_END)
            break;
        else if (status != Z_OK)
        {
            fclose(pInfile);
            fclose(pOutfile);
            return "Compress status error";
        }
    }

    if (deflateEnd(&stream) != Z_OK)
    {
        fclose(pInfile);
        fclose(pOutfile);
        return "Compress stream error";
    }
    if (EOF == fclose(pOutfile))
    {
        fclose(pInfile);
        fclose(pOutfile);
        return "Compress output EOF";
    }
    fclose(pInfile);
    fclose(pOutfile);
    return "OK";
}

string zipUncompress(string src)
{
    int fl = src.length();
    for (int i = 0; i < fl; i++)
        if (src[i] == '\\')
        {
            src.insert(i, "\\\\");
            i += 2;
        }
    for (int i = src.length() - 1; i >= 0; i--)
        if (src[i] == '.')
        {
            name = src.substr(0, i);
            type = src.substr(i + 1);
            break;
        }

    FILE *pInfile, *pOutfile;

    char* srcChar = (char*)src.data();
    pInfile = fopen(srcChar, "rb");
    if (!pInfile)
    {
        fclose(pInfile);
        return "No file input";
    }
    // Determine input file's size.
    fseek(pInfile, 0, SEEK_END);
    long file_loc = ftell(pInfile);
    fseek(pInfile, 0, SEEK_SET);

    string compFile = name;
    char *lf = (char *)compFile.data();
    pOutfile = fopen(lf, "wb");
    if (!pOutfile)
    {
        fclose(pOutfile);
        fclose(pInfile);
        return "No file output";
    }
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = s_inbuf;
    stream.avail_in = 0;
    stream.next_out = s_outbuf;
    stream.avail_out = BUF_SIZE;
    // Uncompression
    unsigned int infile_remaining = (unsigned int)file_loc;

    if (inflateInit(&stream))
    {
        fclose(pOutfile);
        fclose(pInfile);
        return "inflateInit() failed!\n";
    }

    for ( ; ; )
    {
        int status;
        if (!stream.avail_in)
        {
            unsigned int n = my_min(BUF_SIZE, infile_remaining);

            if (fread(s_inbuf, 1, n, pInfile) != n)
            {
                fclose(pOutfile);
                fclose(pInfile);
                return "Uncompress read error";
            }

            stream.next_in = s_inbuf;
            stream.avail_in = n;
            infile_remaining -= n;
        }

        status = inflate(&stream, Z_SYNC_FLUSH);

        if ((status == Z_STREAM_END) || (!stream.avail_out))
        {
            unsigned int n = BUF_SIZE - stream.avail_out;
            if (fwrite(s_outbuf, 1, n, pOutfile) != n)
            {
                fclose(pInfile);
                fclose(pOutfile);
                return "Uncompress write error";
            }
            stream.next_out = s_outbuf;
            stream.avail_out = BUF_SIZE;
        }

        if (status == Z_STREAM_END)
            break;
        else if (status != Z_OK)
        {
            fclose(pInfile);
            fclose(pOutfile);
            return "Uncompress status error";
        }
    }

    if (deflateEnd(&stream) != Z_OK)
    {
        fclose(pInfile);
        fclose(pOutfile);
        return "Uncompress end error";
    }
    if (EOF == fclose(pOutfile))
    {
        fclose(pInfile);
        fclose(pOutfile);
        return "Uncompress EOF";
    }
    fclose(pInfile);
    fclose(pOutfile);
    return "OK";
}

#endif // ZIP_H
