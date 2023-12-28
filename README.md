# File Watcher
This is a simple file watcher class.<br>
It simply keeps track of the files that are given to the class<br>
and let you know if any of the files are modified<br>

# Example
<h4>Create an instance:</h4>

```cpp
LogianApi::Watcher watcher;
```

<h4>Add the files:</h4>

```cpp
watcher.add("important_file.txt");
watcher.add("another_important_file.json");
```

<h4>Watch the files:</h4>

```cpp
watcher.watch([](const Watcher::FileChangeResult& result) {
    switch (result.action)                // action is 'Modify' if result.added and result.removed are both filled
    {
        case Watcher::Action::Add:
            break;
        case Watcher::Action::Remove:
            break;
        case Watcher::Action::Modify:
            break;
        case Watcher::Action::None:       // this implies that file was modified but it's contents remain the same
            break;
    }
});
```

<h4>FileChangeResult struct:</h4>
This struct contains information about modified file.<br>

```cpp
struct FileChangeResult 
{
    std::string filename;         // name of the modified file
    std::string previousValue;    // pre-modification contents of file
    std::string currentValue;     // post-modification contents of file
    std::string added;            // what was added to the file
    std::string removed;          // what was removed from the file
    Action action;                // action that was done { Add, Remove, Modify, None }
};
```

