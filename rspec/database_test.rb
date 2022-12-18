## SQLのリストをテスト実行するrbファイル
RSpec.describe 'database' do
    def run_script(commands)
        raw_output = nil
        IO.popen("./build/coredb", "r+") do |pipe|
            commands.each do |command|
                pipe.puts command
            end
  
            pipe.close_write
  
            # 出力を読み込む
            raw_output = pipe.gets(nil)
        end
        raw_output.split("\n")
    end
  
    # Assertionの記述
    # [TestCase1]
    it 'test inserts and retrieves a row' do
        result = run_script([
            "insert 1 user1 person1@example.com",
            "select",
            ".exit",
        ])
        expect(result).to match_array([
            "TABLE_MAX_PAGES: 100",
            "SimpleDatabase > Executed.",
            "SimpleDatabase > (1, user1, person1@example.com)",
            "Executed.",
            "SimpleDatabase > Thanks for using simple database. Bye!",
        ])
    end

    # [TestCase2]
    it 'test prints error message when table is full' do
        script = (1..1401).map do |i|
          "insert #{i} user#{i} person#{i}@example.com"
        end
        script << ".exit"
        result = run_script(script)
        expect(result[-2]).to eq('Error: Table is full.')
    end
    
    # [TestCase3]
    it 'test prints error message if strings are too long' do
        long_username = "a"*35
        long_email = "a"*256
        script = [
          "insert 1 #{long_username} #{long_email}",
          "select",
          ".exit",
        ]
        result = run_script(script)
        expect(result).to match_array([
            "TABLE_MAX_PAGES: 100",
            "SimpleDatabase > String is too long.",
            "SimpleDatabase > Executed.",
            "SimpleDatabase > Thanks for using simple database. Bye!",
        ])
    end
end

