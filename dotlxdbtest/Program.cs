using System;
using System.Runtime.InteropServices;

class Program
{
  [DllImport("libdotLXDB.dll", CallingConvention = CallingConvention.Cdecl)]
  private static extern IntPtr ReadDB(string filepath);

  [DllImport("libdotLXDB.dll", CallingConvention = CallingConvention.Cdecl)]
  private static extern void FreeString(IntPtr ptr);

  static private void PrintDbJson()
  {
    IntPtr ptr = ReadDB("db.lxdb");
    string? result = Marshal.PtrToStringAnsi(ptr);
    Console.WriteLine(result);
    FreeString(ptr);
  }

  static void Main()
  {
    PrintDbJson();
  }

}