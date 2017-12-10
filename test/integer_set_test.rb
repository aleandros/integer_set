require 'test_helper'

class IntegerSetTest < Minitest::Test
  def test_that_it_has_a_version_number
    refute_nil ::IntegerSet::VERSION
  end
end
