# StandAloneEnumerator Usage Guide
StandAloneEnumerator is a command-line tool for enumerating files in a directory using different strategies. This guide aims to help you understand how to use the tool and its available options.

## Usage
To use StandAloneEnumerator, open a command prompt or terminal and run the executable file with appropriate command line arguments.

```
StandAloneEnumerator.exe --path <directory_path> --strategy <strategy_name> --threads <thread_count>
```
Replace <directory_path> with the path of the directory you want to enumerate.

Replace <strategy_name> with one of the following strategies:

*enumerate_wstring*: Enumerate files in the directory using wide string support.
*enumerate_string*: Enumerate files in the directory using narrow string support.
*enumerate_parallel_wstring*: Enumerate files in the directory using wide string support with parallel processing.
*enumerate_parallel_string*: Enumerate files in the directory using narrow string support with parallel processing.

Replace <thread_count> with the number of threads you want to use for parallel processing (applicable only for parallel strategies).

Options
--path <directory_path>: Specifies the directory path to enumerate.
--strategy <strategy_name>: Specifies the enumeration strategy to use.
--threads <thread_count>: Specifies the number of threads for parallel processing.

## Example
```
StandAloneEnumerator.exe --path C:\MyDirectory --strategy enumerate_parallel_wstring --threads 4
```
This command will enumerate files in the directory C:\MyDirectory using the parallel wide string strategy with 4 threads.

## Notes
Ensure that the directory path provided exists and is accessible.
Choose the enumeration strategy and thread count based on your system resources and performance requirements.
