class FdHandler
{
    int fd;
    bool own_fd;

public:
    FdHandler(int a_fd, bool own = true) 
        : fd(a_fd), own_fd(own) {}
    virtual ~FdHandler();

    virtual void Handle(bool r, bool w) = 0;
    int GetFd() const { return fd; }
    virtual bool WantRead() const { return true; }
    virtual bool WantWrite() const { return false; }
};
