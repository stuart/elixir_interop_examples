defmodule Complex do
	@moduledoc """
		An example of NIF usage. This calls native functions from
		the Gnu Scientific Library's complex number code.
	"""
	@on_load :init

	def init do
		:erlang.load_nif("./priv/bin/complex_nif", 0)
	end

	@doc """
		Returns an opaque reference to the complex number a + ib

		## Example

		iex> ref = Complex.new({1.0, 1.2})
		iex> is_reference(ref)
		true

	"""
	def new({a,b})do
		new(a, b)
	end

  @doc """
		Returns an opaque reference to the complex number a + ib
		## Example

		iex> ref = Complex.new(1.0, 3.4)
		iex> is_reference(ref)
		true

		iex> _ref = Complex.new(1.0, 4)
		** (ArgumentError) argument error

	"""
	def new(_a,_b) do
		exit(:nif_library_not_loaded)
	end

	@doc """
		Returns a tuple representing the complex number held in the reference.
		## Example

		iex> ref = Complex.new(1.0, 3.4)
		iex> Complex.to_tuple(ref)
		{1.0, 3.4}

	"""
	def to_tuple(_complex) do
		exit(:nif_library_not_loaded)
	end

	@doc """
		Returns the absolute value (magnitude or modulus) of a complex number.

		## Example

			iex> ref = Complex.new(1.0, 1.0)
			iex> Complex.abs(ref)
			1.4142135623730951

      iex> Complex.abs("a")
      ** (ArgumentError) argument error

	"""
	def abs(_complex) do
		exit(:nif_library_not_loaded)
	end

	@doc """
		Returns the argument (angle or phase) of a complex number.

		## Example

		iex> ref = Complex.new(1.0, 1.0)
		iex> Complex.arg(ref)
		0.7853981633974483

		iex> Complex.arg(1.23)
		** (ArgumentError) argument error
	"""
	def arg(_complex) do
		exit(:nif_library_not_loaded)
	end
end
