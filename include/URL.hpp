#ifndef URL_H
#define URL_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

namespace nectar::core
{
    enum class Level
    {
        NONE,
        ROOT,
        DEVICE,
        MODULE,
        NODE,
        TAG
    };

    class URLPart
    {
        private:
            unsigned int _machine = 0;
            string _string = "";
            Level _level = Level::NONE;
        
        public:
            URLPart() { }
            URLPart(unsigned int machine, string name, Level lvl)
            {
                _machine = machine;
                _string = name;
                _level = lvl;
            }
            operator unsigned int() const { return _machine; }
            operator string() const { return _string; }
            operator Level() const { return _level; }
            bool operator== (URLPart const &obj) const;

    };

    typedef struct urlparts
    {
        shared_ptr<URLPart> device = make_shared<URLPart>();
        shared_ptr<URLPart> module = make_shared<URLPart>();
        shared_ptr<URLPart> node = make_shared<URLPart>();
        shared_ptr<URLPart> tag = make_shared<URLPart>();
        shared_ptr<URLPart> max;
    } urlparts;

    class URL
    {
        private:
            string _URLString;
            urlparts _parts;
            //what commands does the url accept
            bool _run = false;
            bool _read = false;
            bool _write = false;

        public:
            URL(urlparts &parts);
            ~URL() { };
            void setRun();
            void setRead();
            void setWrite();
            bool isRunnable() const;
            bool isReadable() const;
            bool isWriteable() const;

        public:
            /**
             * @brief Returns the string represention of the URL
             */
            operator string() const { return _URLString; }
            /**
             * @brief Return the max level URL machine code
             */
            operator unsigned int() const { return *_parts.max; }
            urlparts getParts() const;
            bool operator ==(URL const &obj) const { return (_URLString.compare(obj) == 0); }
    };

}


/* Hash function for the URL class; need to insert into unordered_set */
namespace std
{
    using namespace nectar::core;

    template<>
    struct hash<URL>
    {
        size_t operator()(const URL &obj) const
        {
            return hash<string>()((string) obj);
        }
    };

}

#endif