defmodule InteropExamplesTest do
  use ExUnit.Case
  {:ok, _pid} = Node.start(:test@localhost, :shortnames)
  Node.set_cookie :chocolatecookie

  doctest InteropExamples
  doctest Complex
  doctest EiPortExample
  doctest PortDriverExample
  doctest PortExample
  doctest CNodeExample
end
