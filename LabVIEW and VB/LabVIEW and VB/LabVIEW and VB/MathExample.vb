Public Class MathExample

    'This is where you define any data members the class contains.
    Private _number1 As Integer
    Public _number2 As Double

    Public Sub New(ByVal number1 As Integer, ByVal number2 As Double)
        ' This is the constructor, where the class is initialized.
        _number1 = number1
        _number2 = number2
    End Sub

    ' This function takes in two doubles (number1 and number2)
    ' and returns number1*number2.
    Public Function multiply(ByVal number1 As Double, ByVal number2 As Double) As Double
        multiply = number1 * number2
    End Function

    Public Function add(ByVal number1 As Integer, ByVal number2 As Integer) As Integer
        add = number1 + number2
    End Function

    ' You can also take in other data types from LabVIEW besides , such as strings.
    ' This function returns the length of a given string multiplied by
    ' _number1 and _number2.
    Public Function specialStringLength(ByVal someText As String) As Integer
        specialStringLength = CInt(someText.Length() * _number1 * _number2)
    End Function

    ' You can also take in arrays...
    Public Function addArrayElements(ByRef numbers As Integer()) As Integer

        Dim result As Integer
        For index = 0 To numbers.GetUpperBound(0) Step 1
            result = result + numbers(index)
        Next

        addArrayElements = result
    End Function

    ' Or return arrays...
    Public Function buildArray(ByVal number As Integer) As Integer()
        buildArray = New Integer() {number}
    End Function

    ' If you want to return multiple numbers from the same function, you can.
    ' The following function takes in three numbers and returns the cosine
    ' of each number.

    ' Note that each of the inputs hold the respective return values. The return
    ' value for the function itself just tells you that everything succeeded.

    Public Function cosine(ByRef number1 As Double, ByRef number2 As Double, ByRef number3 As Double) As Boolean

        Try
            number1 = Math.Cos(number1)
            number2 = Math.Cos(number2)
            number3 = Math.Cos(number3)

        Catch ex As Exception
            cosine = False
        End Try

        cosine = True
    End Function

End Class
