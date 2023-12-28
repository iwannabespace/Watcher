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