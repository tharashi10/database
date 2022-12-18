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
end

