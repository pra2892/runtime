// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.


.assembly extern System.Runtime.Extensions { auto }
.assembly extern xunit.core {}
.assembly extern mscorlib { auto }
.assembly extern TestLibrary {}

.assembly 'mcc_i80' {}


.namespace MCCTest
{
  .class public MyClass
  {
    .method assembly static pinvokeimpl("native_i8c" as "#1" cdecl)
        vararg valuetype MCCTest.VType8 Sum(float64, int32, int64, float32, int16, float64) cil managed preservesig {
    }

    .method public static int32 Main()
    {
      .custom instance void [xunit.core]Xunit.FactAttribute::.ctor() = (
          01 00 00 00
      )
      .entrypoint
      .maxstack  1
      call       bool [TestLibrary]TestLibrary.PlatformDetection::get_IsWindows()
      brtrue.s   DoTest
      ldc.i4     100
      ret

    DoTest:
      call       int32 MCCTest.MyClass::Test()
      ret
    }

    .method public static int32 Test()
    {
      .maxstack  64
      .locals init	(
        [0] valuetype MCCTest.VType8 res,
        [1] valuetype MCCTest.VType8 v1,
        [2] valuetype MCCTest.VType8 v2,
        [3] valuetype MCCTest.VType8 v3,
        [4] valuetype MCCTest.VType8 v4,
        [5] valuetype MCCTest.VType8 v5,
        [6] valuetype MCCTest.VType8 v6,
        [7] valuetype MCCTest.VType8 v7,
        [8] valuetype MCCTest.VType8 v8,
        [9] valuetype MCCTest.VType8 v9,
        [10] valuetype MCCTest.VType8 v10,
        [11] valuetype MCCTest.VType8 v11,
        [12] valuetype MCCTest.VType8 v12,
        [13] int32 rc
      )

      // Initialize v1 thru v12
      ldloca.s   v1
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v2
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v3
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v4
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v5
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v6
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v7
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v8
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v9
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v10
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v11
      call       instance void MCCTest.VType8::Init()
      ldloca.s   v12
      call       instance void MCCTest.VType8::Init()

      ldc.r8     5
      ldc.i4     1
      ldc.i8     2
      ldc.r4     -1
      ldc.i4     -3
      conv.i2
      ldc.r8     8
      ldloc.s    v1
      ldloc.s    v2
      ldloc.s    v3
      ldloc.s    v4
      ldloc.s    v5
      ldloc.s    v6
      ldloc.s    v7
      ldloc.s    v8
      ldloc.s    v9
      ldloc.s    v10
      ldloc.s    v11
      ldloc.s    v12
      call   vararg valuetype MCCTest.VType8 MCCTest.MyClass::Sum(float64, int32, int64, float32, int16, float64, ...,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8,
                                                                valuetype MCCTest.VType8)

      stloc.s    res

      // Check Result
      ldloc.s    res
      ldc.i4     12
      call       int32 MCCTest.Common::CheckResult(valuetype MCCTest.VType8, int32)
      stloc.s    rc

      ldloc.s    rc
      ret
    } // end of method MyClass::Test

  } // end of class MyClass

} // end of namespace MCCTest
