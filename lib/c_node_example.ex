defmodule CNodeExample do
  @doc """
    Returns the square of the argument.

    ## Example

    iex> CNodeExample.square(3)
    9

  """
  def square(x) do
    call_cnode({:square, x})
  end

  @doc """
    Returns the cube of the argument.

    ## Example

    iex> CNodeExample.cube(3)
    27

  """
  def cube(x) do
    call_cnode({:cube, x})
  end

  def call_cnode(msg) do
    send({:any, :c1@cannelloni}, {:call, self(), msg})
    receive do
      {:cnode, result} ->
        result
    after
      1000 ->
        {:error, :timeout}
    end
  end
end
